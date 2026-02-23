IMAGE_NAME := nativebuildapd:jammy
LOCALUSER_NAME := $(shell id -un)
TARGET_MACHINE := $(or $(MACHINE),RadarFusionMachine)
CONTAINER_NAME := nativebuildapd_$(LOCALUSER_NAME)_$(TARGET_MACHINE)
NETWORK_NAME := ap-native-network
NETWORK := $(or $(NETWORK),docker)
WORKSPACE := $(CURDIR)/..
ifeq ($(TARGET_MACHINE),FusionMachine)
IP := 192.168.7.4
DLT_HOST_PORT := 34904
else
IP := 192.168.7.2
DLT_HOST_PORT := 34902
endif

show:
	@echo "IP $(IP) MACHINE $(TARGET_MACHINE)"

help:
	@echo 'Simple Makefile to help with creating a useful, local, docker container'
	@echo ''
	@echo 'Targets are:'
	@echo '	build:      	Build the image.'
	@echo '	create_network: Create a local docker network.'
	@echo '	dock:           Creates a container and places you in bash as yourself,'
	@echo '                        with your workspace directory mounted to /mnt.'
	@echo '	clean_ucm_data: Clean the ucm temporary data.'
	@echo '	debug:          Debug the executable.
	@echo '	stop:           Stops the container.'
	@echo '	remove_network: Remove the local docker network.'

build:
	docker build \
		--build-arg uid=$(shell id -u) \
		--tag $(IMAGE_NAME) \
		$(WORKSPACE)/yocto-layers/docker/native/

build_sep:
	docker build \
		--build-arg uid=$(shell id -u) \
		--build-arg installseparate=1 \
		--tag $(subst :,_separate:,$(IMAGE_NAME)) \
		$(WORKSPACE)/yocto-layers/docker/native/

build_dev:build
	docker build \
		--tag $(subst :,_devel:,$(IMAGE_NAME)) \
		docker

create_network:
	docker network inspect $(NETWORK_NAME)  > /dev/null 2>&1 || \
	docker network \
		create \
		--driver=bridge \
		--subnet=192.168.7.0/24 \
		$(NETWORK_NAME)

ifeq ($(NETWORK),docker)
backend: create_network
	docker run \
		--rm \
		--ip 192.168.7.11 \
		--net=$(NETWORK_NAME) \
		--name hawkbit_backend \
		-p 8080:8080 \
		--detach \
		hawkbit/hawkbit-update-server:0.3.0

dock: create_network
	docker run \
		-it     \
		--rm \
		--ip $(IP) \
		--net=$(NETWORK_NAME) \
		--volume $(WORKSPACE)/../../:/mnt \
		--name $(CONTAINER_NAME) \
		--workdir /mnt/sources/adaptive-platform-24-11/native-docker-build \
		--cap-add=SYS_PTRACE \
		--cap-add=NET_ADMIN \
		--cap-add=SYS_NICE \
		--env TARGET_MACHINE=$(TARGET_MACHINE) \
		-p $(DLT_HOST_PORT):3490 \
		$(IMAGE_NAME) || true
itm: IMAGE_NAME := $(subst :,_devel:,$(IMAGE_NAME))
itm:	create_network
	docker run \
		--rm \
		--ip $(IP) \
		--net=$(NETWORK_NAME) \
		--volume $(WORKSPACE):/mnt \
		--name $(CONTAINER_NAME) \
		--workdir /mnt/source/adaptive-platform-24-11/native-docker-build \
		--cap-add=SYS_PTRACE \
		--cap-add=NET_ADMIN \
		--cap-add=SYS_NICE \
		--env TARGET_MACHINE=$(TARGET_MACHINE) \
		-p $(DLT_HOST_PORT):3490 \
		$(IMAGE_NAME) \
		/mnt/native-docker-build/itm/docker_itm.sh $(LOGDIR) $(RUN)
else
dock: remove_network
	docker run \
		-it     \
		--rm \
		--net=$(NETWORK) \
		--volume $(WORKSPACE):/mnt \
		--name $(CONTAINER_NAME) \
		--workdir /mnt/native-docker-build \
		--cap-add=SYS_PTRACE \
		--cap-add=SYS_NICE \
		--env TARGET_MACHINE=$(TARGET_MACHINE) \
		$(IMAGE_NAME) || true
backend:
	docker run \
		--rm \
		--net=$(NETWORK) \
		--name hawkbit_backend \
		--detach \
		hawkbit/hawkbit-update-server
endif

dock_dev: IMAGE_NAME := $(subst :,_devel:,$(IMAGE_NAME))
dock_dev: dock

dock_sep: IMAGE_NAME := $(subst :,_separate:,$(IMAGE_NAME))
dock_sep: CONTAINER_NAME := $(CONTAINER_NAME)_separate
dock_sep: dock

remove:
	docker container rm $(CONTAINER_NAME)

remove_network:
	docker network rm $(NETWORK_NAME) || true

stop:
	docker stop $(CONTAINER_NAME) || true

# Targets below this line are intended to be run from within the container

# Clear UCM runtime data. Necessary to rerun the ITM sequence
clean_ucm_data:
	rm -f /tmp/apd/opt/package_manager/var/swpackage_current_status_database.json 
	rm -f /tmp/apd/opt/package_manager/var/swpackage_transfer_status_database.json
	rm -rf /tmp/apd/var/lib/apd_ucm/*
	/mnt/native-docker-build/docker_build.sh

# run the execution manager from the install directory
runapx:
	execution-manager ARA_ROOT=/tmp/apd/ &

# run the machine state and function group cycle
fgchange:
	/mnt/yocto-layers/meta-ara/recipes-apd/wrapper/files/itm-smtool.sh


.PHONY: build stop

