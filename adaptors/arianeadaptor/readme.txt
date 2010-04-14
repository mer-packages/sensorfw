arianeadapter integration to sensord
7.5.2009


environment:
	libusb-0.1.12-15.fc9.i386		USB support
	Qt 4.4.3				support libraryes
	Linux version 2.6.27.7-53.fc9.i686	Linux Fedora 9


build:
	qmake && make


arianeadapter_test.h 
	includes headers for arianeadapter
	defines class for module testing
	defines signals & slots for control 


arianeadapter_test.cpp     		     
	implements sensord_manager


module is created new ArianeProtocol(this, pipe_fd[1])
       this is pointer to a sensord_manager
       pipe_fd[1] is input buffer notification pipe descriptor used to notify arianeadapter of pending input buffer from filter


new session to a sensor is created by signal:

  void api_open_req(int sid, int buffer, int access_mode);

  sid  			is sensor id
  buffer 		buffer samples are outputted (filter), currently placeholder only
  access_mode 		level of control to sensor


mainloop is entered by calling
  run();


no threads nor locks are used at a moment
