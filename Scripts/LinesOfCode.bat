cd ../
git ls-files | grep -P ".*(hpp|cpp|ecs)" | xargs wc -l
cd Scripts/
read -p "Press enter to exit."