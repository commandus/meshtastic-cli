# Attention! Check this hardcoded path!
$GRPC_PLUGIN = "c:\bin\grpc_cpp_plugin.exe"
# -------------------------- Do not edit below this line --------------------------
$WORK_DIR = $($args[0])
$PROTOBUF_INC = "C:\git\vcpkg\installed\x64-windows\include"
$DEST = "."
$GEN = "gen"
$DT = Get-Date -F "yyyy-MM-dd hh:mm"
$GEN_HDR = "$GEN/meshtastic/mesh.pb.h $GEN/meshtastic/mqtt.pb.h"
$GEN_SRC="$GEN/meshtastic/mesh.pb.cc $GEN/meshtastic/mqtt.pb.cc"
$GEN_FILES="$GEN_HDR $GEN_SRC"
$PROTO = "meshtastic/*.proto"
$H = "$GEN/meshtastic/mesh.pb.h"

if ($null -ne $WORK_DIR) {
    cd $WORK_DIR
}

function Add-Proto {
    param (
        $OutputDir, $IncludeDir, $ProtoFile
    )
    # Seriaization
    protoc -I $IncludeDir --cpp_out=$OutputDir $ProtoFile
}

# generate protobuf
Add-Proto -OutputDir $GEN -IncludeDir "third-party/proto" -ProtoFile "nanopb.proto"

Get-ChildItem -Path "third-party/proto/meshtastic" -Filter *.proto |
Foreach-Object {
    Add-Proto -OutputDir $GEN -IncludeDir "third-party/proto" -ProtoFile "meshtastic/$_"
}
