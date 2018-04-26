#!/bin/sh

systemctl --system daemon-reload
systemctl restart ovip-sys-hmi-application
