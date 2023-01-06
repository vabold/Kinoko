import csv
from dataclasses import dataclass


def read_csv_file(file_name: str):
    inputs = []
    with open(file_name, "r") as f:
        reader = csv.reader(f)
        for line in reader:
            if len(line) < 6:
                raise Exception(f"Malformed line in csv file: {line}")
            inputs.append(
                [
                    int(line[0]),  # A
                    int(line[1]),  # B
                    int(line[2]),  # Item
                    int(line[4]) + 7,  # Y-Analog
                    int(line[3]) + 7,  # X-Analog
                    int(line[5]),  # D-Pad
                ]
            )


@dataclass
class MetaData:
    TrackId: int
    Character: int
    Vehicle: int
    ManualDrift: bool


def get_dummy_data():
    """Generate dummy metadata to put in the header of the rkg file"""
    return MetaData(0, 0, True)


def write_rkg(inputs: list, metadata: MetaData):
    """Writes an uncompressed RKG file with a minimal header
    More information on the RKG format: https://wiki.tockdom.com/wiki/RKG_(File_Format)
    """

    # The header of the rkg file
    header = [0x00] * 0x88
    # header magic
    header[0:4] = [ord("R"), ord("K"), ord("G"), ord("D")]
    # TrackId
    header[0x7] = metadata.TrackId << 0x2
    # Vehicle
    header[0x8] = metadata.Vehicle << 0x2
    # Character
    header[0x8] = header[0x8] & (metadata.Character & 0x30)
    header[0x9] = metadata.Character << 0x4
    # Drift
    drift_type = 0 if metadata.ManualDrift else 1
    header[0xD] = header[0xD] | drift_type << 2

    # The input section
    face_buttons = []
    direction_inputs = []
    trick_inputs = []

    for input in inputs:
        


if __name__ == "__main__":
    read_csv_file("test.csv")
