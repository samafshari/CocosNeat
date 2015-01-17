import Image
import os
import fnmatch
import json
import shutil

sizes = [29, 40, 50, 57, 58, 72, 76, 80, 100, 114, 120, 144, 152,
         16, 32, 48, 64, 128, 256, 512]

def copy_files(src, dst):
    for item in os.listdir(src):
        path = os.path.join(src, item)
        # Android can not package the file that ends with ".gz"
        if not item.startswith('.') and not item.endswith('.gz') and os.path.isfile(path):
            shutil.copy(path, dst)
        if os.path.isdir(path):
            new_dst = os.path.join(dst, item)
            os.mkdir(new_dst)
            copy_files(path, new_dst)

def copy_resources(app_android_root, allowedDirs):
    if not os.path.isdir(app_android_root):
        print "{} not found.".format(app_android_root)
        return

    # remove app_android_root/assets if it exists
    assets_dir = os.path.join(app_android_root, "assets")
    if os.path.isdir(assets_dir):
        shutil.rmtree(assets_dir)

    # copy resources
    os.mkdir(assets_dir)
    resources_dir = os.path.join(app_android_root, "../Resources")
    if os.path.isdir(resources_dir):
        copy_files(resources_dir, assets_dir)

    for directory in os.walk(assets_dir).next()[1]: #list all directories copied
        if directory not in allowedDirs:
            shutil.rmtree(os.path.join(assets_dir,directory))


def makeIcon(iconPath, outDir, sizes=sizes, aa=True):
    img = Image.open(iconPath)
    for size in sizes:
        resized = img.resize((size,size), Image.ANTIALIAS if aa else Image.NEAREST)
        imgname = os.path.join(outDir, "Icon-" + str(size) + ".png")
        resized.save(imgname)
        print "Wrote " + imgname
    print "makeIcon finished successfully."

#makeIcon("Design/Icon1024.png", "proj.ios_mac/ios")
#makeIcon("Design/Icon1024-t.png", "Design/transIcons")

print "Writing Android Icons..."
configDb = json.loads(open("Design/config.json", "r").read())
for androidIcon in configDb["android"]:
    img = Image.open(os.path.join("Design/AndroidIcons", androidIcon["icon"]))
    for dest in androidIcon["dest"]:
        img.resize((36,36), Image.ANTIALIAS).save(os.path.join(dest, "res/drawable-ldpi/icon.png"))
        img.resize((48,48), Image.ANTIALIAS).save(os.path.join(dest, "res/drawable-mdpi/icon.png"))
        img.resize((72,72), Image.ANTIALIAS).save(os.path.join(dest, "res/drawable-hdpi/icon.png"))
        img.resize((96,96), Image.ANTIALIAS).save(os.path.join(dest, "res/drawable-xhdpi/icon.png"))
        print "Wrote android icons for " + dest

print "Writing IOS Icons..."
for iosIcon in configDb["iosIcons"]:
    img = Image.open(iosIcon["source"])
    iosFolder = os.path.join("proj.ios_mac/ios", iosIcon["dest"])
    aa = "Nearest" not in iosIcon or not iosIcon["Nearest"]
    if not os.path.isdir(iosFolder):
        os.mkdir(iosFolder)
    for size in sizes:
        resized = img.resize((size,size), Image.ANTIALIAS if aa else Image.NEAREST)
        imgname = os.path.join(iosFolder, "Icon-" + str(size) + ".png")
        resized.save(imgname)
        print "Wrote " + imgname
    img.resize((1024,1024), Image.ANTIALIAS).save(os.path.join("Design/iOS_Large", iosIcon["dest"] + "_1024.jpg"))

print "Copying Resources..."
resourcesDir = "Design/xd"
for root, dir, files in os.walk(resourcesDir):
    for item in fnmatch.filter(files, "*.png"):
        outDir = "Resources/"
        if root != resourcesDir:
            #make directories
            directoryName = root.replace("\\","/").split("/")[-1]
            outDir = os.path.join(outDir, directoryName)
        if not os.path.isdir(outDir): 
            os.mkdir(outDir)
            os.mkdir(os.path.join(outDir, "xd"))
            os.mkdir(os.path.join(outDir, "hd"))
        img = Image.open(os.path.join(root, item))
        print "Read " + os.path.join(root, item)
        img.resize(img.size, Image.ANTIALIAS).save(os.path.join(outDir, "xd", item))
        print "Wrote " + os.path.join(outDir, "xd", item)
        img.resize((img.size[0] / 2, img.size[1] / 2), Image.ANTIALIAS).save(os.path.join(outDir, "hd", item))
        print "Wrote " + os.path.join(outDir, "hd", item)

resourcesDir = "Design/img"
for root, dir, files in os.walk(resourcesDir):
    for item in fnmatch.filter(files, "*.png"):
        img = Image.open(os.path.join(resourcesDir, item))
        img.resize(img.size, Image.ANTIALIAS).save("Resources/img/" + item)
        print "Wrote " + item

print "Resource conversion finished successfully."

print "Distributing Android resources..."
sharedFolders = configDb["androidShared"]
for androidAsset in configDb["android"]:
    allowedDirs = sharedFolders + (androidAsset["include"] if "include" in androidAsset else [])
    for dest in androidAsset["dest"]:
        print "Distributing Android assets for " + dest
        copy_resources(dest, allowedDirs)
print "Android Asset distribution completed."