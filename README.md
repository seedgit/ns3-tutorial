# NS3 Tutorial

## Quick setup environment (using docker)
1. Download and install Visual studio code from [https://code.visualstudio.com/Download](https://code.visualstudio.com/Download)
2. Download and install Docker Desktop from [https://www.docker.com/products/docker-desktop](https://www.docker.com/products/docker-desktop)
3. Download ns3 from [https://www.nsnam.org/releases/](https://www.nsnam.org/releases/)
4. Extract compress file to working directory
5. Make new Visual Studio Code workspace with this 3 direcoty
	* ns-allinone-3.29/ns-3.29/src
	* ns-allinone-3.29/ns-3.29/example
	* ns-allinone-3.29/ns-3.29/scratch
<figure>
  <img src="img/fig01-vs-workspace.png" alt="alt text" title="my tooltip">
  <figcaption>What it should look like in Visual Studio Code</figcaption>
</figure>

6. Open Terminal in Visual Studio Code. And select current working directory to ``ns-allinone-3.29/ns-3.29/scratch``
7. For OSX run 

	```
	docker run --rm -it -w /usr/ns-allinone-3.26/ns-3.26 -v `pwd`:/usr/ns-allinone-3.26/ns-3.26/scratch ryankurte/docker-ns3
	```
8. For Window
	
	```
docker run --rm -it -w /usr/ns-allinone-3.26/ns-3.26 -v C:\path-to-scratch-directory:/usr/ns-allinone-3.26/ns-3.26/scratch ryankurte/docker-ns3
	```
9. In Docker environment, your current directory will be ``/usr/ns-allinone-3.26/ns-3.26``
	
	
## Make your own coding
1. Write your own code in ``scratch`` directory
2. ``.cc`` is type for ``C Language``
3. Run your code by ``./waf --run scratch/your-filename-without-file-type``. For example,

	```
	./waf --run scratch/scratch-simulator
	```
4. All code in scratch must not contain error
5. Basic code should look like this

```c
#include "ns3/core-module.h"

using namespace ns3;
int 
main (int argc, char *argv[])
{
  //Setup, config, install application and etc.
  Simulator::Run ();
  Simulator::Destroy ();
}

```
