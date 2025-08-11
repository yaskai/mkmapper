dir_name=$(pwd)
shopt -s extglob
proj_name=${dir_name%%+(/)}
proj_name=${proj_name##*/}   
proj_name=${proj_name:-/}    

cd ./build &&
./premake5 gmake2 &&
cd .. &&
make &&
./bin/Debug/$proj_name
