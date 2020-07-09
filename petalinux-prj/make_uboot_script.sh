helpFunction(){
    echo "";
    echo "Usage: \t$0 [-i <input_file>] [-o <output_file] [-n <name_of_script>]";
    echo "";
    exit 1;
}

while getopts ":i:o:n:" opt; do
    case ${opt} in
        i) inputfile=$OPTARG
           ;;
        o) outputfile=$OPTARG
           ;;
        n) name=$OPTARG
           ;;
        *) helpFunction
          ;;
    esac
done

echo "";
echo "===== Generating u-boot script =====";
mkimage -T script -C none -n "$name" -d "$inputfile" "images/linux/$outputfile";
echo "";
echo "Script generated: images/linux/$outputfile";
exit 1;
