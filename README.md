# LSM_Project

We have conducted a case study on two major CVE exploits on Linux systems:
1. CVE-2022-0185
2. CVE-2022-0492

Both of the exploits depend on using the unshare command to gain unfair access to a root level namespace and to escape a docker container. We first performed these exploits on our systems and then created a Linux security module using eBPF to prevent the exploit. The complete report can be found in this github repository. 
