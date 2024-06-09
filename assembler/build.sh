g++ ./src/main.cpp -o ./bin/main
g++ -DDEBUG ./src/main.cpp -o ./bin/debug
chmod +x ./bin/main
chmod +x ./bin/debug