# Container Escape Prevention using eBPF

We have conducted a case study on two major CVE exploits on Linux systems:
1. CVE-2022-0185
2. CVE-2022-0492

Both of the exploits depend on using the unshare command to gain unfair access to a root level namespace and to escape a docker container. We first performed these exploits on our systems and then created a Linux security module using eBPF to prevent the exploit. The complete report can be found in this github repository. 

## About Namespaces 

In order to give users more control over how resources are utilized and shared, Linux offers namespaces
that allow the isolation of certain resources, including process IDs, network interfaces, file systems,
and more. For containerization technologies like Docker and LXC, they now serve as the foundation.
For instance, the PID namespace isolates the process ID space, allowing each process to have its own
set of process IDs that are distinct from those used by processes in other namespaces. When using
containerization, when different containers must be kept separate from one another while running on
the same host, this can be helpful.


The owner may act as a ”root” inside the USER namespace. It offers a way to separate user
and group rights. Restricting processes from accessing resources outside of their assigned namespace,
it adds an additional degree of protection. A process is given a special set of user and group IDs
that are different from the user and group IDs in the parent namespace when it enters a new user
namespace. As a result, processes executing in a user namespace are unable to access resources or
carry out operations that demand authorization from a different namespace. User namespaces can
be used to manage a variety of system components, including network interfaces, mount points, and
interprocess communication (IPC) resources, in addition to separating user and group identities. User
namespaces are a potent tool for creating secure and segregated environments in Linux systems as a
result of this.

## Container Escape 

The term “Container Escape” refers to the event where a malicious or legitimate user is able to escape the container isolation and access resources (e.g. filesystem, processes, network interfaces) on the host machine.

The two previously mentioned exploits leverage the unshare command to escape the current container and gain unfair access to a root-level namespace.

Using unshare command:

![image](https://github.com/omkmorendha/LSM_Project/assets/17925053/86bbcc72-09e3-4a36-b025-5b51ff8d8a94)

## CVE-2022-0185
This flaw, which affects the filesystem context functionality in Linux kernel versions 5.1-rc1 and
up, was found on January 18, 2022. The ”legacy parse param” function, specifically, contains a
heap-based buffer overflow that makes it possible for an attacker to write outside the boundaries of a
kernel memory buffer.

Bypassing any Linux namespace limitations that may be in place, exploitation of this issue could
grant an unprivileged attacker root rights. This might provide the attacker access to the compromised
machine and let them carry out nefarious deeds. With a score of 7.8, the seriousness of this vulnerability
is rated High.

The Linux kernel development team corrected the issue, and on February 4, 2022, the Linux
kernel version 5.16.1 was released with the fix.

## CVE-2022-0492
This flaw, which is a logical error in how cgroups are implemented, could be used by an attacker to
escalate their privileges on the system or obtain unauthorized access to system resources. Resource
management and distribution among processes are made possible by Cgroups. They are frequently
used to limit and distribute system resources among several containers in containerization technologies
like Docker and Kubernetes. With a score of 7.0, the vulnerability’s seriousness is graded as High.

## Our solution 
![image](https://github.com/omkmorendha/LSM_Project/assets/17925053/8d09eacb-83f6-47ee-b679-9d763e20967b)

We have created an eBPF module that that follows the above mentioned flowchart, to prevent the exploits.

## Loading the Module

By using the MakeFile available in this repository we can load our eBPF Module:
![image](https://github.com/omkmorendha/LSM_Project/assets/17925053/89934730-bb66-42e5-9ce7-5be4a5872ca6)

## Results

1. Without Module
  ![image](https://github.com/omkmorendha/LSM_Project/assets/17925053/1105ad6f-d439-47d5-acf7-be1abfe09ab6)


2. With Module
   ![image](https://github.com/omkmorendha/LSM_Project/assets/17925053/f59ee0a5-a2c7-4402-944f-b08b8888a9af)
