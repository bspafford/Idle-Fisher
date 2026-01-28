import os

def list_png_files(root_dir, output_file):
    with open(output_file, 'w') as f:
        for dirpath, dirnames, filenames in os.walk(root_dir):
            for filename in filenames:
                if filename.lower().endswith('.png'):
                    full_path = os.path.join(dirpath, filename)
                    rel_path = full_path.replace("\\", "/")
                    rel_path = rel_path.replace(".PNG", ".png")
                    rel_path = rel_path.replace("Q:/Documents/VisualStudios/idleFisher/idleFisher", ".")
                    f.write(f"{rel_path.lower()}\n")  # Write the absolute path

def list_png_files1(root_dir, output_file):
    with open(output_file, 'a') as f:
        for dirpath, dirnames, filenames in os.walk(root_dir):
            for filename in filenames:
                if filename.lower().endswith('.png'):
                    full_path = os.path.join(dirpath, filename)
                    rel_path = full_path.replace("\\", "/")
                    rel_path = rel_path.replace("Q:/Documents/VisualStudios/idleFisher/idleFisher", ".")
                    rel_path = rel_path.replace(".PNG", ".png")
                    f.write(f"{(rel_path.lower())}\n")  # Write the absolute path
        
        f.write("\n")
        f.write("None".lower())

list_png_files("Q:/Documents/VisualStudios/idleFisher/idleFisher/images", "Q:/Documents/VisualStudios/idleFisher/idleFisher/data/png_files.txt")
list_png_files1("Q:/Documents/VisualStudios/idleFisher/idleFisher/fonts", "Q:/Documents/VisualStudios/idleFisher/idleFisher/data/png_files.txt")