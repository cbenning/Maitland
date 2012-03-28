Maitland: A prototype paravirtualization-based packed malware detection system for Xen virtual machines
Copyright (C) 2011 Christopher A. Benninger

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.


--------------------------------------------
Maitland
--------------------------------------------

Maitland is a prototype implementation of my thesis work. It is a paravirtualization-based framework for Xen virtual machine introspection. The existing code is an example of using this framework to detect and unpack packed or encrypted processes in remote virtual machines using only a simple Python wrapper for grep to detect signatures.

As this is my thesis work, it contains some of the experiment scripts I wrote to help me out. I haven't trimmed many of the files which arent being used either.

Disclaimer: This software is very unstable in its current state and as a prototype, works only in a very specifically setup environment. 

--------------------------------------------
Important Directories:
--------------------------------------------

    benchmark/
    Contains all experiment scripts and data.

    dom0/
    Contains all user-space code which is intended to run in domain0.

    dom0/python_server
    Contains the main Python helper daemon that exists in Maitland's architecture.

    domU/
    Contains all user-space code which is intended to run in domainU VMs

    domU/python_client
    Contains the main user-space Python interface to Maitland that lives in domainU. This is for experimentation only.

    drivers/
    Contains all kernel driver code

    drivers/xen/malpage
    Contains all kernel driver code intended to live in each domainU kernel.

    drivers/xen/monitor
    Contains all kernel driver code intended to live in the monitor domain0 VM kernel.

    linux-2.6-pvops.git
    Contains the slightly-modified 2.6.32 linux kernel I used for development against and for deployment.

    packers/
    Contains the sample packers used in my evaluation.

    scripts/
    Contains all helper scripts for compilation, deployment and setup of the environment.


--------------------------------------------
Compilation:
--------------------------------------------

In the root folder, just run: make

--------------------------------------------
Deployment:
--------------------------------------------

Customize the scripts in scripts/dom0 and scripts/domU for your setup.

The general procedure to make everything run is:

1. Distribute all scripts and compiled modules to the VMs
2. Load dom0 module and fire up the helper Python daemon
3. Load domU module which will register automatically
4. Run process with python client which will auto-watch the new process ID

Either watch grep detect signatures, or watch the VM crash... :D



