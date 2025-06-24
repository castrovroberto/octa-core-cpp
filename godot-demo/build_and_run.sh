#!/bin/bash

echo "🚀 Building Octa-Core Hexagonal Honeycomb Demo for Stakeholders"
echo "============================================================="

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

cd ..

echo -e "${BLUE}Step 1: Building C++ shared library...${NC}"
cd build/Release
if cmake -DBUILD_GODOT_BINDING=ON . && make octacore-godot; then
    echo -e "${GREEN}✅ C++ library built successfully${NC}"
else
    echo -e "${RED}❌ Failed to build C++ library${NC}"
    exit 1
fi

echo -e "${BLUE}Step 2: Copying library to Godot project...${NC}"
cd ../../godot-demo
cp ../build/Release/liboctacore.dylib lib/
if [ $? -eq 0 ]; then
    echo -e "${GREEN}✅ Library copied successfully${NC}"
else
    echo -e "${RED}❌ Failed to copy library${NC}"
    exit 1
fi

echo -e "${BLUE}Step 3: Hexagonal honeycomb demo ready!${NC}"
echo ""
echo -e "${YELLOW}📋 Demo Instructions:${NC}"
echo "1. Open Godot 4.2+"
echo "2. Import this project (godot-demo folder)"
echo "3. Run the project (F5 or Play button)"
echo ""
echo -e "${YELLOW}🎯 Stakeholder Demo Points:${NC}"
echo "• Show the stunning hexagonal honeycomb layout"
echo "• Click hexagons to demonstrate arrow rotation (60°)"
echo "• Trigger honeycomb chain reactions"
echo "• Highlight the 6-directional system (original format)"
echo "• Show recursive contagion through honeycomb paths"
echo "• Emphasize this is the canonical 'Hex Contagion' game"
echo ""
echo -e "${GREEN}🎮 Hexagonal honeycomb demo is ready for stakeholder presentation!${NC}"

# Optional: Launch Godot if available
if command -v godot &> /dev/null; then
    echo ""
    read -p "Launch Godot now? (y/n): " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        echo -e "${BLUE}Launching Godot...${NC}"
        godot .
    fi
fi 