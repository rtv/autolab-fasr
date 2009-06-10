#!/bin/bash

echo "/** \mainpage Autolab Fast and Frugal Sustain and Resupply"
./Markdown.pl ../README
echo "**/"

echo "/** \page install Installation"
./Markdown.pl ../INSTALL
echo "**/"

echo "/** \page release Release Notes"
./Markdown.pl ../RELEASE
echo "**/"

echo "/** \page authors Authors"
./Markdown.pl ../AUTHORS
echo "**/"

echo "/** \page copying Copying"
./Markdown.pl ../COPYING
echo "**/"
