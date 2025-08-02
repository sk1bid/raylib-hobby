#!/usr/bin/env bash
set -euo pipefail

HOST="skbid@192.168.0.100"
REMOTE_TMP="/tmp/site-root"
REMOTE_ROOT="/var/www/site-root"

echo "[1/3] Sync to temporary folder on server"
# Note: space between source and destination; exclude unnecessary files
echo "rsync -avz --delete --exclude='.git' --exclude='*.sh' --exclude='*.md' ./ $HOST:$REMOTE_TMP/"
rsync -avz --delete \
      --exclude='.git' \
      --exclude='*.sh' \
      --exclude='*.md' \
      ./ "$HOST:$REMOTE_TMP/"

echo "[2/3] Move to web-root and fix permissions"
# Use unquoted EOF so local vars expand; remote bash sees literal paths
ssh "$HOST" bash <<EOF
sudo rsync -avz --delete --exclude='.git' $REMOTE_TMP/ $REMOTE_ROOT/
sudo chown -R www-data:www-data $REMOTE_ROOT
sudo rm -rf $REMOTE_TMP
EOF

echo "[3/3] Done!"
