for i in `seq 1 2`;
do
	x-terminal-emulator -x "timeout 2; netcat 127.0.0.1 8081"
done
