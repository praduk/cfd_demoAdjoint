SRCS := $(wildcard *.cc)
OBJS := $(patsubst %.cc,obj/%.o,$(SRCS))
CXXFLAGS := -O3 -DNDEBUG
LDFLAGS := -g -lm -lcairo -lz -lX11


demo: $(OBJS)
	@echo Linking demo
	@g++ $(CXXFLAGS) $(OBJS) $(LDFLAGS) -o demo
	@mkdir -p movie

-include obj/*.d

obj :
	@mkdir obj

obj/%.o : %.cc Makefile | obj
	@echo Compiling $<
	@g++ -MM -MT $@ $(CXXFLAGS) $< -o $(@:.o=.d)
	@g++ -c $(CXXFLAGS) -o $@ $<

clean:
	@rm -rf obj demo movie/*.png *.png
