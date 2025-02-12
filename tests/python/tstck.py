#
# Invoke STCK instruction
#
# To run:
#
# $ STEPLIB=MACHO.PDSE64 python tstck.py
#
import ctypes
stck = ctypes.CDLL("STCK")
value = ctypes.create_string_buffer(8)
ticks = ctypes.create_string_buffer(8)
x = stck.STCK(value, ticks)
iticks = int.from_bytes(ticks, byteorder='big')
print("STCK = ", end="")
for i in value.raw:
	print(f"{i:02X}", end="")
print("")
print("ticks=", iticks / 4096.0, "microseconds")