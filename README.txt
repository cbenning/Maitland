





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

    reference/
    Contains reference work which isn't used directly but I needed to hang onto.

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



