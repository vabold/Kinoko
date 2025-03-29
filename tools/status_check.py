# Validate that the framecounts in STATUS.md are up-to-date. This is done by trying to run the
# entire test case and asserting that the sync framecount is the highest it currently can be. This
# helps us capture when changes focused on one test case actually benefit a different test case.

import json
import os
import subprocess
import sys
from typing import Dict, TypedDict
from generate_tests import generate_tests

STATUS_TEST_CASE_FILENAME = 'statusTestCases.json'


class TestCaseDict(TypedDict):
    rkgPath: str
    krkgPath: str
    targetFrame: int


class TestCase:
    def __init__(self, line: str):
        self.line = line
        self.name = self.get_name_from_line()
        self.emoji = self.get_emoji_from_line()
        self.has_description = bool(self.get_description_from_line())

        frame_counts = self.get_frame_counts_from_line()
        self.sync_frame = int(frame_counts[0])
        self.total_frames = int(frame_counts[1])

    def get_name_from_line(self) -> str:
        # Get the test case name in between the square brackets
        return self.line[self.line.find('[') + 2:self.line.find(']') - 1]

    def get_emoji_from_line(self) -> str:
        return self.line.split('|')[2].strip()

    def get_description_from_line(self) -> str:
        return self.line.split('|')[-1].rstrip('\r\n')

    def get_frame_counts_from_line(self) -> list[str]:
        # Get the frame counts in between the pipes
        return self.line.split('|')[1].split('/')

    # While normally "targetFrame" would just be the lower bound, we want to run the upper bound
    # and see if it produces a later desync than we currently are tracking in STATUS.md
    def to_json(self) -> TestCaseDict:
        return {
            "rkgPath": "samples/" + self.name + ".rkg",
            "krkgPath": "samples/" + self.name + ".krkg",
            "targetFrame": self.total_frames
        }


class KinokoOutputValidator:
    def __init__(self, stdout):
        self.test_cases = {}

        for line in stdout.split('\n'):
            # Skip any desync debugging lines
            if "Test Case" not in line:
                continue

            self.current_line = line
            name = self.get_test_case_name()
            desync_frame = self.get_test_case_frame_lower_bound()
            self.test_cases[name] = desync_frame

    # e.g. Get "rr-ng-rta-2-24-281" from [TestDirector.cc:112] REPORT: Test Case
    # Passed: rr-ng-rta-2-24-281 [1207 / 9060]
    def get_test_case_name(self) -> str:
        return self.current_line.split(':')[3].split(' ')[1]

    # e.g. Get the "814" from
    # [TestDirector.hh:55] REPORT: Test Case Failed: rr-ng-rta-2-24-281 [814 / 9060]
    def get_test_case_frame_lower_bound(self) -> int:
        return int(self.current_line.split(':')[3].split('[')[1].split(' ')[0])

    def validate_test_case(self, test_case: TestCase) -> bool:
        desync_frame = self.test_cases[test_case.name]

        case_name_just = test_case.name.ljust(20)

        if desync_frame == test_case.total_frames:
            if test_case.sync_frame == test_case.total_frames:
                if test_case.emoji != '✔️':
                    # Wrong emoji. It should be a checkmark.
                    print(f"{case_name_just}\t❌\t->\t✔️\t[EMOJI]")
                    return False

                if test_case.has_description:
                    # There should be no desync reason for a synced test case.
                    print(
                        case_name_just + \
                        "\tSynchronized runs should not have a desync reason [REASON]")
                    return False
                return True

            # We have a synced test case, but the label is incorrect
            msg = [
                case_name_just, test_case.sync_frame,
                "->", test_case.total_frames, "[LABEL]"
            ]
            print("\t".join(msg))
            return False

        if desync_frame - 1 == test_case.sync_frame:
            if test_case.emoji != '❌':
                # Wrong emoji. It should be an x.
                print(f"{case_name_just}\t✔️\t->\t❌\t[EMOJI]")
                return False

            if not test_case.has_description:
                # There should be a desync reason if we haven't synced the full test case.
                print(
                    f"{case_name_just}\tDesynchronized runs should have a desync reason [REASON]")
                return False
            return True

        # Our framecount isn't up-to-date - the test case desyncs either later or
        # earlier than captured in STATUS.md
        early_or_late = "EARLY" if (desync_frame - 1 < test_case.sync_frame) else "LATER"
        print(
            f"{case_name_just}\t{test_case.sync_frame}\t->\t{desync_frame - 1}\t[{early_or_late}]")

        return False


def get_test_cases_from_status_md() -> Dict[str, TestCase]:
    test_cases = {}
    # Read STATUS.md and parse test cases to dictionary
    with open('STATUS.md', 'r', encoding='utf-8') as f:
        # Skip the first 4 lines to get to first test case line
        for _ in range(4):
            next(f)

        for line in f:
            test_case = TestCase(line)
            test_cases[test_case.name] = test_case
    return test_cases


def validate_results(test_cases: Dict[str, TestCase], result: str) -> bool:
    is_success = True
    validator = KinokoOutputValidator(result)

    for test_case in test_cases.values():
        is_success &= validator.validate_test_case(test_case)

    return is_success


def create_json_file(test_cases: Dict[str, TestCase]):
    # Write out the test cases to a JSON file
    test_case_dict = {}

    for key, value in test_cases.items():
        test_case_dict[key] = value.to_json()

    with open(STATUS_TEST_CASE_FILENAME, 'w', encoding="utf-8") as f:
        f.write(json.dumps(test_case_dict, indent=4))


def main():
    # Generate the upper bound test case JSON
    test_cases = get_test_cases_from_status_md()

    create_json_file(test_cases)

    # Generate the binary file
    generate_tests(STATUS_TEST_CASE_FILENAME)

    # Run Kinoko. subprocess.run shell behavior varies between Windows and Linux
    exec = os.path.join('.', 'kinoko') # pylint: disable=redefined-builtin
    args = [
        exec,
        "-m",
        "test",
        "-s",
        "testCases.bin",
    ] if sys.platform.startswith('win32') else exec + " -m test -s testCases.bin"
    result = subprocess.run(
        args, cwd='out', shell=True, capture_output=True, text=True, check=False
    )

    # Check each test case is up-to-date. If not, return non-zero exit code so
    # our build action is aware.
    if not validate_results(test_cases, result.stdout):
        sys.exit(1)


if __name__ == '__main__':
    main()
