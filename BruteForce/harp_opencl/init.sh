### The developer should confirm the dir names before starts
### This snippet can be put in .bashrc or similar, to avoid execution every session

export AALSDK="/usr/local/include/aalsdk"
export ALTERAOCLSDKROOT="$HOME/intel/altera_pro/16.0/hld"
export QUARTUS_ROOTDIR_OVERRIDE="$ALTERAOCLSDKROOT/../quartus"
export QSYS_ROOTDIR="$HOME/intel/altera_pro/16.0/quartus/sopc_builder/bin"
export AOCL_BOARD_PACKAGE_ROOT="$ALTERAOCLSDKROOT/board/bdw_fpga_pilot_opencl_bsp_v1.0.2"
export PATH=$ALTERAOCLSDKROOT/bin:$AALSDK/bin:$QUARTUS_ROOTDIR_OVERRIDE/bin:$PATH
export LD_LIBRARY_PATH=/usr/local/lib:/usr/local/lib64:/usr/lib:/usr/lib64:$ALTERAOCLSDKROOT/host/linux64/lib:$AOCL_BOARD_PACKAGE_ROOT/linux64/lib:$AOCL_BOARD_PACKAGE_ROOT/host/linux64/lib/:$AALSDK/lib:$LD_LIBRARY_PATH
export LM_LICENSE_FILE="$HOME/intel/license.dat"
export ALTERAD_LICENSE_FILE="$HOME/intel/license.dat"
source $ALTERAOCLSDKROOT/init_opencl.sh
