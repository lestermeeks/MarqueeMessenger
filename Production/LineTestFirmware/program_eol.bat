echo "Place device in DFU Mode.  Hold Setup, tap reset, wait for yellow, release setup."
PAUSE
particle update
TIMEOUT 5
particle flash --serial eol.bin
TIMEOUT 5
particle identify
particle serial wifi

echo "particle device add [deviceid]"

echo "record device id for adding to product later"
pause
