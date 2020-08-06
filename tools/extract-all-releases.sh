#!/bin/sh
# curl -s https://api.github.com/repos/ossia/libossia/releases/29384453 | grep browser_download_url | awk '{ print $2 }'  | sed 's/"//g' | xargs -n 1 -P 16 wget

for file in *.tar.gz ; do
  FOLDER=`echo $file | sed 's/.tar.gz//g'` 
  mkdir $FOLDER
  mv $file $FOLDER/
  (
  cd $FOLDER
  tar xaf $file
  )
done
for file in *.zip ; do
  FOLDER=`echo $file | sed 's/.zip//g'` 
  mkdir $FOLDER
  mv $file $FOLDER/
  (
  cd $FOLDER
  7z x $file
  )
done