windir=/mnt/d/ESP/flash_download_tool/temp
cmakefile=./CMakeLists.txt
builddir=./build

# 检查windows端目标文件夹是否存在
if [ ! -d "$windir" ]; then
  echo "| 目标文件夹: ${windir} 不存在, 请检查"
  exit 1
fi

# 检查最外侧CMakeLists.txt文件是否存在
if [ ! -f "$cmakefile" ]; then
  echo "| CMakeLists.txt 文件不存在, 请检查"
  exit 1
fi

# 检查build目录是否存在
if [ ! -d "$builddir" ]; then
  echo "| build目录不存在, 请检查"
  exit 1
fi

#! 清空windows端目标文件夹
rm -f ${windir}/**.bin
echo "| 目标文件夹: ${windir} 已清空"

#! 提取项目名称
target=$(grep -E "^project\(" "$cmakefile" | sed -E 's/project\(([^)]+)\)/\1/')
target=$(echo $target | tr -d '\r')
echo "| 项目名称: $target"

#! 从第二行开始计算行数
lines=$(awk 'NR>1 {print $0}' $builddir/flash_args | wc -l)
echo "| 镜像文件数量: $lines"

#! 从第二行开始读取第一列参数
addr_list=$(awk 'NR>1 {print $1}' $builddir/flash_args)

#! 从第二行开始, 读取第二列参数
file_list=$(awk 'NR>1 {print $2}' $builddir/flash_args)

#! 读取每一行的参数,并复制到windows端
for i in $(seq 1 $lines)
do
  addr=$(echo $addr_list | awk -v i=$i '{print $i}')
  file=$(echo $file_list | awk -v i=$i '{print $i}')
  filename=$(basename "$file")
  if [ "$filename" = "$target.bin" ];then
    wfilename="main.bin"
  else
    wfilename=$filename
  fi
  filepath=${builddir}/${file}
  wfilepath=${windir}/${wfilename}
  if [ -f $filepath ]; then
    cp $filepath ${wfilepath}
    printf "| %-20s >> %-20s : %-10s\n" $filename $wfilename $addr
  fi
done