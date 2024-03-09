Before running the program, please ensure that gcc has been successfully installed in your environment. If you are a Linux user, simply execute "make" to compile the CAD_tool.c according to the makefile. Additionally, the input file is located in the cloud, please download it on your own.

input file ===>https://drive.google.com/drive/folders/16A3gVJxT8NeKYr4lyPY2j8s2t7dESO19?usp=drive_link

/////Execution File//////
type following command in the terminal
c17.v & c17.pat are the verilog file describe the circuit, test_lib is the liberty file .
$./CAD_tool c17.v c17.pat test_lib.lib 

Then you will get three extra file illustate the delay load and power consumption of specific input file.

Golden file can help user to check the correctness of their design 
