#!/bin/bash
######################################################
# 88Q5192 10base-t1s Multi Drop Mode Init Seq
# 2024. 07. 30
# Release Node Rev. 0.2
# Set Port 13,14,15 (Die1, Port3,4,5)
#
# This Ver only Port Num, Node ID and Node Count can be set up.
#
# phytool write IF/Die/-/-
# IF - pfe1, pfe1, eth0
# Die0 - 0xf - pfe2 / Die1 - 0xe - pfe1
# 
# phytool write clause 45
# e.g - IF pfe1, Die 0, Port 4, Dev 1, Reg 0x0834, Data 0xc001
# phytool write pfe2/0xf/0x19 0x0834 (Reg)
# phytool write pfe2/0xf/0x18 0x8
# phytool write pfe2/0xf/0x19 0xc001 (Data)
# phytool wrtie pfe2/0xf/0x18 0x8..
######################################################

pn_1=""
node_id=1
max_plca_node=8

# Function to display usage information
usage() {
    echo "Usage: $0 [-n <Port Num>] [-i <Node ID>] [-m <Max PLCA Node>] "
    echo "Options:"
    echo "  -n <Port Num>      Port Num (valid ports: 13, 14, 15)"
    echo "  -i <Node ID>       Node ID (default: 1, range: 0~254)"
    echo "  -m <Max PLCA Node> Max PLCA Node (default: 0x08, range: 1~255)"
    exit 1
}
valid_ports=(13 14 15)

# Process command line options
if [[ $# -eq 0 ]]; then
    usage
fi

while getopts ":n:i:m:" opt; do
    case $opt in
        n)
            pn_1=$OPTARG
            if [[ ! " ${valid_ports[@]} " =~ " $pn_1 " ]]; then
                echo "Error: Invalid port number. Please specify a valid port number (13, 14, 15)."
                exit 1
            fi
        ;;
        i)
            node_id=$OPTARG
            if ! ((node_id >= 0 && node_id <= 254)); then
                echo "Error: Invalid Node ID. Please specify a valid Node ID (0~255)."
                exit 1
            fi
        ;;
        m)
            max_plca_node=$OPTARG
            if ! ((max_plca_node >= 1 && max_plca_node <= 255)); then
                echo "Error: Invalid Max PLCA Node. Please specify a value in the range 0~255."
                exit 1
            fi
        ;;
        \?)
            echo "Error: Invalid option -$OPTARG."
            usage
            exit 1
        ;;
    esac
done

# Determine port-specific variables
case $pn_1 in
    13)
        pn_1="63"
        c22_p="0x8305"
        c22_p_2="0xE305"
        pn_2="7F"
    ;;
    14)
        pn_1="83"
        c22_p="0x8405"
        c22_p_2="0xE405"
        pn_2="9F"
    ;;
    15)
        pn_1="A3"
        c22_p="0x8505"
        c22_p_2="0xE505"
        pn_2="BF"
    ;;
esac

commands_part1=(
    "0xFC27 0x20A0"
    "0xFC26 0x8152"
    "0xFC27 0x20A0"
    "0xFC26 0x8152"
    "0xFC28 0x0440"
    "0xFC27 0x20A1"
    "0xF744 0x1FF8"
    "0xF75F 0xEC01"
    "0xF760 0x6186"
    "0xF75E 0xF816"
    "0xF758 0x330F"
    "0xF759 0x28AB"
    "0xF75A 0xAFFF"
    "0xF766 0x614F"
    "0xF762 0x795E"
    "0xF765 0xB4DC"
    "0xF750 0x0000"
    "0xF761 0x4141"
    "0xF763 0x1990"
    "0xF764 0x6540"
    "0xF75B 0x70FF"
    "0xF70F 0x0094"
    "0xF70E 0x1C69"
)

commands_part2=(
    "0x9000 0x0000"
    "0xF732 0xDF11"
    "0xF733 0x134C"
    "0xF734 0x3526"
    "0xF735 0x3B3C"
    "0xF736 0x0039"
    "0xF750 0x0000"
)

for cmd in "${commands_part1[@]}"; do
    IFS=' ' read -r reg1 reg2 <<< "$cmd"
    phytool write pfe1/0xe/0x19 $reg1
    phytool write pfe1/0xe/0x18 0x80${pn_1}
    phytool write pfe1/0xe/0x19 $reg2
    phytool write pfe1/0xe/0x18 0x84${pn_1}
done

phytool write pfe1/0xe/0x1 0x3000
phytool write pfe1/0xe/0x0 0x977B

phytool write pfe1/0xe/0x1 $c22_p
phytool write pfe1/0xe/0x0 0x9779

phytool write pfe1/0xe/0x1 0x587
phytool write pfe1/0xe/0x0 0x977A

phytool write pfe1/0xe/0x1 $c22_p_2
phytool write pfe1/0xe/0x0 0x9779

phytool write pfe1/0xe/0x19 0x9000
phytool write pfe1/0xe/0x18 0x80${pn_1}
phytool write pfe1/0xe/0x19 0x0000
phytool write pfe1/0xe/0x18 0x84${pn_1}

for cmd in "${commands_part2[@]}"; do
    IFS=' ' read -r reg1 reg2 <<< "$cmd"
    phytool write pfe1/0xe/0x19 $reg1
    phytool write pfe1/0xe/0x18 0x80${pn_1}
    phytool write pfe1/0xe/0x19 $reg2
    phytool write pfe1/0xe/0x18 0x84${pn_1}
done

node_conf=$(printf "0x%02X%02X" $max_plca_node $node_id)

# Node Cnt/ ID
phytool write pfe1/0xe/0x19 0xCA02
phytool write pfe1/0xe/0x18 0x80${pn_2}
phytool write pfe1/0xe/0x19 $node_conf # Node Cnt, ID
phytool write pfe1/0xe/0x18 0x84${pn_2}

# To Timer - 32bit
phytool write pfe1/0xe/0x19 0xCA04
phytool write pfe1/0xe/0x18 0x80${pn_2}
phytool write pfe1/0xe/0x19 0x0020
phytool write pfe1/0xe/0x18 0x84${pn_2}

# PLCA Enable
phytool write pfe1/0xe/0x19 0xCA01
phytool write pfe1/0xe/0x18 0x80${pn_2}
phytool write pfe1/0xe/0x19 0x8000
phytool write pfe1/0xe/0x18 0x84${pn_2}

