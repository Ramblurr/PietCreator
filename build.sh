mkdir build
cd build
cmake ../
make

if [ ! -e pietcreator ] 
then
  echo "pietcreater not found"
  exit 1
fi