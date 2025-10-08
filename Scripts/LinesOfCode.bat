echo "Файлы .ecs:"
cd ../

read -p "Press enter to exit."
find . -name "*.ecs" -type f -exec wc -l {} + | tail -1

echo "Файлы .cpp:"
find . -name "*.cpp" -type f -exec wc -l {} + | tail -1

echo "Файлы .hpp:"
find . -name "*.hpp" -type f -exec wc -l {} + | tail -1

cd Scripts/
read -p "Press enter to exit."