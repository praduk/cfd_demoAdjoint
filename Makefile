SRCS := $(wildcard *.cc)
OBJS := $(patsubst %.cc,obj/%.o,$(SRCS))
CXXFLAGS := -O3 -DNDEBUG
LDFLAGS := -lm -lcairo


demo: $(OBJS)
	@echo Linking demo
	@g++ $(CXXFLAGS) $(OBJS) -o demo

-include obj/*.d

obj :
	@mkdir obj

obj/%.o : %.cc Makefile | obj
	@echo Compiling $<
	@g++ -MM -MT $@ $(CXXFLAGS) $< -o $(@:.o=.d)
	@g++ -c $(CXXFLAGS) -o $@ $<

clean:
	@rm -rf obj demo
