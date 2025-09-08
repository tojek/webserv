<?php
$target_dir = $UPLOAD_DIR;
$target_file = $target_dir . basename($_FILES["files"]["name"]);

if (move_uploaded_file($_FILES["files"]["tmp_name"], $target_file)) {
    echo "The file ". basename($_FILES["files"]["name"]). " has been uploaded.";
} else {
    echo "Sorry, there was an error uploading your file.";
}
?>