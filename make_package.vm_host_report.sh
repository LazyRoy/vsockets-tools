#!/bin/bash

echo "### Building package ..."

rm -Rf target/package/*
cp -R bin.aux  target/package/
cp  target/linux/bin/*  target/package/bin.aux/linux32

cp -R src/scripts/*.rb target/package

cd target

tar zcvf tarballs/vmhost_report.tar.gz package/*