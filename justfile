default: build-run

clean:
    cmake --build build --target clean
build:
    cmake -B build && cmake --build build
run:
    ./build/out

clean-build: clean build

build-run: build run

clean-build-run: clean build run

alias c := clean
alias b := build
alias r := run
alias cb := clean-build
alias br := build-run
alias cbr := clean-build-run
