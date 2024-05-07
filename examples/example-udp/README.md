This example returns any UDP packet to its sender.

Test with netcat via:
```shell
# Set IP and port according to your configuration.
echo "Hello World" | nc -u -w1 192.168.2.10 5000
```