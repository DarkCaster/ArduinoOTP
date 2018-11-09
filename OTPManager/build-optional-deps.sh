#!/bin/bash

script_dir="$( cd "$( dirname "$0" )" && pwd )"

check_error () {
if [ "$?" != "0" ]; then
	echo "Build failed"
	exit 1
fi
}

# update submodules

git submodule update --init --recursive
check_error

rm -f "$script_dir/DotNetBlocks/NuGet.Config"
check_error

rm -f "$script_dir/NuGet.Config"
check_error

# build all external deps for DotNetBlocks

"$script_dir/DotNetBlocks/External/DotNetBuildTools/prepare-and-build.sh" "$script_dir/DotNetBlocks/External"
check_error

cp "$script_dir/DotNetBlocks/External/DotNetBuildTools/dist/extra/NuGet.Config" "$script_dir/DotNetBlocks/NuGet.Config"
check_error

chmod 644 "$script_dir/DotNetBlocks/NuGet.Config"
check_error

# use DotNetBuildTools sub-submodule from DotNetBlocks submodule and build all external deps for this project

"$script_dir/DotNetBlocks/External/DotNetBuildTools/prepare-and-build.sh" "$script_dir/External"
check_error

cp "$script_dir/External/NuGet.Config" "$script_dir/NuGet.Config"
check_error

chmod 644 "$script_dir/NuGet.Config"
check_error

