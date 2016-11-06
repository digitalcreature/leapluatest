leaplua.so: libleaplua.cpp
	$(CXX) $? -o $@ -shared -fPIC -lLeap
