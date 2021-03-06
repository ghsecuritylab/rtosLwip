=========================
Usage of Python utilites
=========================

run './run.py'

::
 ./run.py --help
 ./run.py COMMAND --help, 

for example, `./run.py httpOs --help
     

Usage
--------

1. Search all nodes in LAN:
::

 ./run search -d

2. Lookup one node info:
::

 run find $IP


3. Update firmware 
 * by HTTP
::

	run httpOs --help
	run httpOs 192.168.168.130
	run httpFpga 192.168.168.130

 * by TFTP
::

  run tftpOs 192.168.168.130
  run tftpFpga 192.168.168.130



Send JSON IP command to node
::

 ./run setParams '{"cName":"JackLeeRX01"}' 192.168.168.60 -d

 ./run setParams '{"ip":"192.168.168.121","isDhcp":1}' 192.168.168.120 -d

 ./run setParams '{"IsConnect":0}' 192.168.168.60 -d

 ./run setParams '{"isDipOn":0}' 192.168.168.60 -d
 ./run setParams '{"isDipOn":1}' 192.168.168.60 -d

 ./run setParams '{"MCASTip":"239.100.1.61"}' 192.168.168.60 -d


 ./run setParams '{"RS232Baudrate":115200}' 192.168.168.60 -d

./run setParams '{"RS232Parity":"even"}' 192.168.168.60 -d
./run setParams '{"RS232Stopbits":0}' 192.168.168.60 -d
./run setParams '{"RS232Databits":7}' 192.168.168.60 -d




 ./run rs232 11223344 192.168.168.60

About RS232 command format:

 string "11223344": every char is used as nibble of one hexadecimal, so send 0x11, 0x22("), 0x33(3), and 0x44(D);
 so "4142434445" will send ABCDE in RS232;

::
 ./run rs232 4142434445 192.168.168.60
