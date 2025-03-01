#
# Invoke EXTRACT macro
#
# To run:
#
# $ STEPLIB=MACHO.PDSE64 python textract.py
#
import ctypes
extract = ctypes.CDLL("EXTRACT")
answer = ctypes.create_string_buffer(48)
ticks = ctypes.create_string_buffer(8)
x = extract.EXTRACT(answer, ticks)
iticks = int.from_bytes(ticks, byteorder='big')
print("EXTRACT = ", end="")
for i in answer.raw:
    print(f"{i:02X}", end="")
print("")
print("ticks=", iticks / 4096.0, "microseconds")