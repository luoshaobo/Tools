
TOOL_CHAIN_CONIFIG_FILE = toolChain.cmake

all : tk app

tk : tk_build
	make -C tk_build

tk_build :
	mkdir -p tk_build
	( \
        cd tk_build && \
        cmake -DCMAKE_TOOLCHAIN_FILE=../$(TOOL_CHAIN_CONIFIG_FILE) ../tk \
    )
    
app : app_build
	make -C app_build

app_build :
	mkdir -p app_build
	( \
        cd app_build && \
        cmake -DCMAKE_TOOLCHAIN_FILE=../$(TOOL_CHAIN_CONIFIG_FILE) ../app \
    )

clean:
	rm -fr tk_build app_build
