#!/bin/bash
actions=( document-new document-new document-save document-save-as application-exit view-form-table zoom-in zoom-out transform-scale system-run run-build process-stop edit-undo insert-image)
sizes=( 32 22 16 )
source_path=/usr/share/icons/oxygen

for size in ${sizes[@]}
do
    for icon in ${actions[@]}
    do
        icon_path=$source_path/$size"x"$size/actions/$icon.png
        cp $icon_path ./fallback/$size"x"$size/actions/
    done
done


apps=( utilities-terminal )

for size in ${sizes[@]}
do
    for icon in ${apps[@]}
    do
        icon_path=$source_path/$size"x"$size/apps/$icon.png
        cp $icon_path ./fallback/$size"x"$size/apps/
    done
done
