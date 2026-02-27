#!/bin/bash
search_dir="${1:-.}"
if [ ! -d "$search_dir" ]; then
    echo "Ошибка: директория '$search_dir' не существует." >&2
    exit 1
fi
total_lines=0
file_count=0
while IFS= read -r -d '' file; do
    lines=$(wc -l < "$file" 2>/dev/null | tr -d ' ')
    if [[ "$lines" =~ ^[0-9]+$ ]]; then
        total_lines=$((total_lines + lines))
        file_count=$((file_count + 1))
    else
        echo "Предупреждение: не удалось прочитать '$file'" >&2
    fi
done < <(find "$search_dir" -type f -name "*.ecs" -print0 2>/dev/null)
if [ $file_count -eq 0 ]; then
    echo "Файлы .ecs не найдены."
else
    echo ".ecs files processed: $file_count"
    echo ".ecs lines number: $total_lines"
fi


read -p "Press enter to exit."