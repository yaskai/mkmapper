dirname=$(pwd)
shopt -s extglob
result=${dirname%%+(/)}
result=${result##*/}
result=${result:-/}
echo $result
