import bpy
import os

def save_mesh_data(obj, output_dir):
    """Сохраняет вершины и индексы меша в файлы."""
    if not obj or obj.type != 'MESH':
        print("Ошибка: выберите меш-объект.")
        return None
    
    mesh = obj.data
    
    # 1. Получаем данные (с триангуляцией)
    mesh.calc_loop_triangles()
    vertices = [v.co[:] for v in mesh.vertices]
    triangles = mesh.loop_triangles
    
    # 2. Формируем массив индексов
    indices = []
    for tri in triangles:
        indices.extend(tri.vertices[:])
    
    # 3. Создаем имена файлов (без недопустимых символов)
    base_name = "".join(c for c in obj.name if c.isalnum() or c in (' ', '-', '_')).rstrip()
    
    # 4. Сохраняем вершины
    vert_path = os.path.join(output_dir, f"{base_name}_geom.txt")
    with open(vert_path, 'w', encoding='utf-8') as f:
        f.write(f"shapeGeometryData = {{\n")
        f.write(f"mesh = {{\n")
        f.write(f"\tvertices = {{ \n")
        for v in vertices:
             f.write(f"\t\t{{ x ={v[0]:.6f}, y ={v[1]:.6f}, z ={v[2]:.6f} }},\n")
        f.write(f"\t}},\n")
        
        f.write(f"indices = {{ \n")
        for i in range(0, len(indices), 3):
            f.write(f"\t\t{indices[i]}, {indices[i+1]}, {indices[i+2]},\n")
        f.write(f"}}\n")
        f.write(f"}}\n")
        f.write(f"}}\n")
        
    return vert_path

# === КОНФИГУРАЦИЯ ===
# Путь для сохранения (по умолчанию - рабочий стол Windows)
output_dir = os.path.join(os.path.expanduser("~"), "Desktop")
# Можно изменить на другую папку:
# output_dir = r"C:\Users\ВашеИмя\Documents\3D_Data"

# === ВЫПОЛНЕНИЕ ===
active_obj = bpy.context.active_object
save_mesh_data(active_obj, output_dir)