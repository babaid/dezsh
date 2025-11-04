import os
from hpp2plantuml.hpp2plantuml import CreatePlantUMLFile

header_dir = "include"

headers = []
for root, dirs, files in os.walk(header_dir):
    for file in files:
        if file.endswith((".h", ".hpp")):
            headers.append(os.path.join(root, file))

output_file = "my_project.puml"  # or .uml if you prefer

CreatePlantUMLFile(headers, output_file)
print(f"UML (PlantUML) file generated: {output_file}")

