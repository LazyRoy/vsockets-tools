#!/bin/bash

rm -Rf target/package/*
cp -R bin.aux  target/package/

cp -R src/scripts/*.rb target/package

cd target

tar zcvf tarballs/vmhost_report.tar.gz package/*