CXX = arm-cas-linux-gnueabi-g++

all : sys_flag_mgr_test
  
CFLAGS =  -Wall -Wextra -Werror -I. -I/home/uidv4956/tcam_root/tcam_src_root.latest/release/fs/devel/usr/include
LFLAGS = -lrt -pthread -L/home/uidv4956/tcam_root/tcam_src_root.latest/release/fs/devel/usr/lib/boost -lboost_system -lboost_thread

sys_flag_mgr_test : sys_flag_mgr_test.cpp sys_flag_mgr.cpp sys_flag_mgr_impl.cpp sys_flag_mgr.hpp sys_flag_mgr_impl.hpp
	$(CXX) -DFOR_TEST -pthread $(CFLAGS) -std=c++11 -o sys_flag_mgr_test sys_flag_mgr_test.cpp sys_flag_mgr.cpp sys_flag_mgr_impl.cpp $(LFLAGS)

clean:
	rm -f *.o
	rm -f sys_flag_mgr_test
