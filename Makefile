##########################################################################
# If not stated otherwise in this file or this component's Licenses.txt
# file the following copyright and licenses apply:
#
# Copyright 2017 RDK Management
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
##########################################################################

RM          := rm -rf
CXXFLAGS    = -std=c++1y  -g -fPIC -D_REENTRANT -Wall 
LIBNAME     := ds-hal
LIBNAMEFULL := lib$(LIBNAME).so
OBJS        := $(patsubst %.c,%.o,$(wildcard *.c))

library: $(OBJS)
	@echo "Building $(LIBNAMEFULL) ...."
	$(CXX) $(OBJS) $(CXXFLAGS) $(CFLAGS) -shared -Wl,-soname,lib$(LIBNAME).so -o $(LIBNAMEFULL)

%.o: %.c
	@echo "Building $@ ...."
	$(CXX) -c $<  $(CXXFLAGS) $(CFLAGS) -o $@

install: $(LIBNAMEFULL)
	@echo "Installing files in $(DESTDIR) ..."
	install -d $(DESTDIR)
	install -m 0755 $< $(DESTDIR)
