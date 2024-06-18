import json
import os
import struct

class TestCase:
    def __init__(self, key, value):
        self.name = str.encode(key) + b'\x00'
        self.rkgPath = str.encode("../"+value["rkgPath"]) + b'\x00'
        self.krkgPath = str.encode("../"+value["krkgPath"]) + b'\x00'
        self.targetFrame = value["targetFrame"]

def generate_tests(filename = 'testCases.json'):
    # Parse test cases from JSON
    tests = []

    with open(filename) as f:
        data = json.load(f)

        for key, value in data.items():
            tests.append(TestCase(key, value))

    TEST_CASE_HEADER = b'TSTH'
    TEST_CASE_FOOTER = b'TSTF'

    BINARY_VER_MAJOR = 1
    BINARY_VER_MINOR = 0

    # Generate binary (enforce big-endian)
    os.makedirs("out", exist_ok=True)
    with open('out/testCases.bin', 'wb') as f:
        # Number of test cases
        f.write(struct.pack('>H', len(tests)))

        # Major version
        f.write(struct.pack('>H', BINARY_VER_MAJOR))

        # Minor version
        f.write(struct.pack('>H', BINARY_VER_MINOR))

        for test in tests:
            # Alignment check header
            f.write(struct.pack('>4s', TEST_CASE_HEADER))

            # Total memory size for this test case (excluding header/footer)
            nameLen = len(test.name)
            rkgLen = len(test.rkgPath)
            krkgLen = len(test.krkgPath)
            totalSize = 2 + nameLen + 2 + rkgLen + 2 + krkgLen + 2
            f.write(struct.pack('>H', totalSize))

            # Test name
            f.write(struct.pack('>H', nameLen))
            f.write(struct.pack('>' + str(nameLen) + 's', test.name))

            # RKG
            f.write(struct.pack('>H', rkgLen))
            f.write(struct.pack('>' + str(rkgLen) + 's', test.rkgPath))

            # KRKG
            f.write(struct.pack('>H', krkgLen))
            f.write(struct.pack('>' + str(krkgLen) + 's', test.krkgPath))

            # Total frames of run sync expected
            f.write(struct.pack('>H', test.targetFrame))

            # Alignment check footer
            f.write(struct.pack('<4s', TEST_CASE_FOOTER))
