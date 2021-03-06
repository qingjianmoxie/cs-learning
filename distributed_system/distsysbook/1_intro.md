# 1. Distributed systems at a high level

> Distributed programming is the art of solving the same problem that you can solve on a single computer using multiple computers.

There are two basic tasks that any computer system needs to accomplish(完成):

- storage and
- computation

Distributed programming is the art of solving the same problem that you can solve on a single computer using multiple computers - usually, because the problem no longer fits on a single computer.

Nothing really demands(需要) that you use distributed systems. Given infinite money and infinite R&D time, we wouldn't need distributed systems. All computation and storage could be done on a magic box - a single, incredibly(难以置信的) fast and incredibly reliable system *that you pay someone else to design for you*.

However, few people have infinite resources. Hence, they have to find the right place on some real-world cost-benefit(成本效益) curve. At a small scale, upgrading hardware is a viable(可行的) strategy. However, as problem sizes increase you will reach a point where either the hardware upgrade that allows you to solve the problem on a single node does not exist, or becomes cost-prohibitive. At that point, I welcome you to the world of distributed systems.

It is a current reality that the best value is in mid-range, commodity hardware - as long as the maintenance(维护) costs can be kept down through fault-tolerant software.

Computations primarily benefit from high-end(高端) hardware to the extent to which they can replace slow network accesses with internal memory accesses. The performance advantage of high-end hardware is limited in tasks that require large amounts of communication between nodes.

![cost-efficiency](http://book.mixu.net/distsys/images/barroso_holzle.png)

As the figure above from [Barroso, Clidaras & Hölzle](http://www.morganclaypool.com/doi/abs/10.2200/S00516ED2V01Y201306CAC024) shows, the performance gap between high-end and commodity hardware decreases with cluster size assuming a uniform memory access pattern across all nodes.(假设所有节点的内存访问模式都是一致的，那么高端硬件和普通硬件之间的性能差距会随着集群的增大而减小)

Ideally(理想情况下), adding a new machine would increase the performance(性能) and capacity(容量) of the system linearly. But of course this is not possible, because there is some overhead(开销) that arises due to having separate computers. Data needs to be copied around, computation tasks have to be coordinated(协调) and so on. This is why it's worthwhile to study distributed algorithms - they provide efficient solutions to specific problems, as well as guidance about what is possible, what the minimum cost of a correct implementation is, and what is impossible.

The focus of this text is on distributed programming and systems in a mundane, but commercially relevant setting: the data center.(本文的重点是一个平凡但与商业相关的环境中的分布式编程和系统：数据中心。) For example, I will not discuss specialized problems that arise from having an exotic network configuration(异常网络配置), or that arise in a shared-memory(共享内存) setting. Additionally, the focus is on exploring the system design space rather than on optimizing any specific design - the latter is a topic for a much more specialized text.

## What we want to achieve: Scalability(可扩展性) and other good things

The way I see it, everything starts with the need to deal with size.

Most things are trivial(不重要的) at a small scale - and the same problem becomes much harder once you surpass a certain size, volume or other physically constrained thing. It's easy to lift a piece of chocolate, it's hard to lift a mountain. It's easy to count how many people are in a room, and hard to count how many people are in a country.

So everything starts with size - scalability. Informally speaking, in a scalable system as we move from small to large, things should not get incrementally worse. Here's another definition:

>[Scalability](http://en.wikipedia.org/wiki/Scalability) is the ability of a system, network, or process, to handle a growing amount of work in a capable manner or its ability to be enlarged to accommodate that growth.

What is it that is growing? Well, you can measure growth in almost any terms (number of people, electricity usage etc.). But there are three particularly interesting things to look at:

- Size scalability: adding more nodes should make the system linearly faster; growing the dataset should not increase latency
- Geographic(地理的) scalability: it should be possible to use multiple data centers to reduce the time it takes to respond to user queries, while dealing with cross-data center latency in some sensible manner.(它应该可以使用多个数据中心来减少响应用户查询所需的时间，同时以某种明智的方式处理跨数据中心延迟。)
- Administrative scalability: adding more nodes should not increase the administrative costs(管理成本) of the system (e.g. the administrators-to-machines ratio).

Of course, in a real system growth occurs on multiple different axes(轴) simultaneously; each metric captures just some aspect of growth.

A scalable system is one that continues to meet the needs of its users as scale increases. There are two particularly relevant aspects - performance and availability - which can be measured in various ways.

### Performance (and latency)

>[Performance](http://en.wikipedia.org/wiki/Computer_performance) is characterized by the amount of useful work accomplished by a computer system compared to the time and resources used.

Depending on the context, this may involve achieving one or more of the following:

- Short response time/low latency for a given piece of work
- High throughput(高吞吐量) (rate of processing work)
- Low utilization of computing resource(s)

There are tradeoffs(权衡) involved in optimizing for any of these outcomes. For example, a system may achieve a higher throughput by processing larger batches of work thereby(因此) reducing operation overhead. The tradeoff would be longer response times for individual pieces of work due to batching.(为这些结果中的任何一个进行优化都涉及到权衡。例如，系统可以通过处理更大批量的工作来实现更高的吞吐量，从而减少操作开销。由于分批处理，权衡使得单个工作的响应时间会更长。)

I find that low latency - achieving a short response time - is the most interesting aspect of performance, because it has a strong connection with physical (rather than financial) limitations. It is harder to address(解决) latency using financial resources than the other aspects of performance.

There are a lot of really specific definitions for latency, but I really like the idea that the etymology of the word evokes:
ps:etymology(the origin and history of a particular word)

> Latency  
> The state of being latent; delay, a period between the initiation of something and the occurrence.

And what does it mean to be "latent"?

> Latent  
> From Latin latens, latentis, present participle of lateo ("lie hidden"). Existing or present but concealed(隐藏的) or inactive.

This definition is pretty cool, because it highlights how latency is really the time between when something happened and the time it has an impact or becomes visible.

For example, imagine that you are infected with an airborne virus that turns people into zombies. The latent period is the time between when you became infected, and when you turn into a zombie. That's latency: the time during which something that has already happened is concealed from view.

Let's assume for a moment that our distributed system does just one high-level task: given a query, it takes all of the data in the system and calculates a single result. In other words, think of a distributed system as a data store with the ability to run a single deterministic computation (function) over its current content:

`result = query(all data in the system)`

Then, what matters for latency is not the amount of old data, but rather the speed at which new data "takes effect" in the system. For example, latency could be measured in terms of how long it takes for a write to become visible to readers.

The other key point based on this definition is that if nothing happens, there is no "latent period(潜伏期)". A system in which data doesn't change doesn't (or shouldn't) have a latency problem.

In a distributed system, there is a minimum latency that cannot be overcome: the speed of light limits how fast information can travel, and hardware components(组件) have a minimum latency cost incurred per operation (think RAM and hard drives but also CPUs).(硬件组件(如RAM和硬盘驱动器，以及cpu)的每个操作所产生的最小延迟成本)

How much that minimum latency impacts your queries depends on the nature(性质) of those queries and the physical distance the information needs to travel.

### Availability (and fault tolerance)

The second aspect of a scalable system is availability.

> [Availability](http://en.wikipedia.org/wiki/High_availability)  
> the proportion(比例) of time a system is in a functioning condition(运行状态). If a user cannot access the system, it is said to be unavailable.

Distributed systems allow us to achieve desirable characteristics that would be hard to accomplish on a single system. For example, a single machine cannot tolerate any failures since it either fails or doesn't.(例如，一台机器不能容许任何故障，因为它要么发生故障，要么不发生故障。)

Distributed systems can take a bunch of unreliable components, and build a reliable system on top of them.

Systems that have no redundancy(冗余) can only be as available as their underlying components(基础组件). Systems built with redundancy can be tolerant of partial failures and thus be more available. It is worth noting that "redundant" can mean different things depending on what you look at - components, servers, datacenters and so on.
ps: It is worth noting that 值得注意的是

Formulaically, availability is: `Availability = uptime / (uptime + downtime)`.

Availability from a technical perspective(观点,角度) is mostly about being fault tolerant. Because the probability of a failure occurring increases with the number of components, the system should be able to compensate(补偿) so as to not become less reliable as the number of components increases.

For example:

<table>
<tr>
  <td>Availability %</td>
  <td>How much downtime(停机时间) is allowed per year?</td>
</tr>
<tr>
  <td>90% ("one nine")</td>
  <td>More than a month</td>
</tr>
<tr>
  <td>99% ("two nines")</td>
  <td>Less than 4 days</td>
</tr>
<tr>
  <td>99.9% ("three nines")</td>
  <td>Less than 9 hours</td>
</tr>
<tr>
  <td>99.99% ("four nines")</td>
  <td>Less than an hour</td>
</tr>
<tr>
  <td>99.999% ("five nines")</td>
  <td>~ 5 minutes</td>
</tr>
<tr>
  <td>99.9999% ("six nines")</td>
  <td>~ 31 seconds</td>
</tr>
</table>


Availability is in some sense a much wider concept than uptime(运行时间), since the availability of a service can also be affected by, say, a network outage or the company owning the service going out of business (which would be a factor which is not really relevant to fault tolerance but would still influence the availability of the system). But without knowing every single specific aspect of the system, the best we can do is design for fault tolerance.
在某种意义上，可用性是一个比正常运行时间更广泛的概念，因为服务的可用性还可能受到网络中断或拥有服务的公司停业的影响（这是一个与容错性并不真正相关但仍会影响系统可用性的因素）。但在不了解系统的每一个具体方面的情况下，我们能做的就是设计容错性.

What does it mean to be fault tolerant?

> Fault tolerance  
> ability of a system to behave in a well-defined manner once faults occur

Fault tolerance boils down to(归结为) this: define what faults you expect and then design a system or an algorithm that is tolerant of them. You can't tolerate faults you haven't considered.

## What prevents us from achieving good things?

Distributed systems are constrained by two physical factors:

- the number of nodes (which increases with the required storage and computation capacity)
- the distance between nodes (information travels, at best, at the speed of light)

Working within those constraints:

- an increase in the number of independent nodes increases the probability of failure in a system (reducing availability and increasing administrative costs)
- an increase in the number of independent nodes may increase the need for communication between nodes (reducing performance as scale increases)
- an increase in geographic distance increases the minimum latency for communication between distant nodes (reducing performance for certain operations)

Beyond these tendencies - which are a result of the physical constraints - is the world of system design options.

Both performance and availability are defined by the external guarantees the system makes. On a high level, you can think of the guarantees as the SLA (service level agreement) for the system(性能和可用性都由系统提供的外部保证来定义。从高level角度看，你可以将这些保证看作是系统的SLA（服务级别协议）): if I write data, how quickly can I access it elsewhere? After the data is written, what guarantees do I have of durability(持久性)? If I ask the system to run a computation, how quickly will it return results? When components fail, or are taken out of operation, what impact will this have on the system?

There is another criterion(标准,准则), which is not explicitly mentioned but implied(暗示): intelligibility(可理解性). How understandable are the guarantees that are made? Of course, there are no simple metrics(指标,度量) for what is intelligible.

I was kind of(有点) tempted to put "intelligibility" under physical limitations. After all, it is a hardware limitation in people that we have a hard time understanding anything that involves [more moving things than we have fingers](http://en.wikipedia.org/wiki/Working_memory#Capacity). That's the difference between an error and an anomaly(异常) - an error is incorrect behavior, while an anomaly is unexpected behavior. If you were smarter, you'd expect the anomalies to occur.

## Abstractions and models

This is where abstractions and models come into play. Abstractions make things more manageable by removing real-world aspects that are not relevant to solving a problem. Models describe the key properties of a distributed system in a precise(精确的) manner(方式). I'll discuss many kinds of models in the next chapter, such as:

- System model (asynchronous / synchronous)
- Failure model (crash-fail, partitions, Byzantine)
- Consistency model (strong, eventual)

A good abstraction makes working with a system easier to understand, while capturing the factors that are relevant for a particular purpose.

There is a tension(矛盾,对立) between the reality that there are many nodes and with our desire for systems that "work like a single system". Often, the most familiar model (for example, implementing a shared memory abstraction on a distributed system) is too expensive.

A system that makes weaker guarantees has more freedom of action, and hence potentially(潜在的) greater performance - but it is also potentially hard to reason about. People are better at reasoning about systems that work like a single system, rather than a collection of nodes.

One can often gain performance by exposing(暴露) more details about the internals of the system. For example, in [columnar storage](http://en.wikipedia.org/wiki/Column-oriented_DBMS), the user can (to some extent) reason about the locality of the key-value pairs within the system and hence make decisions that influence the performance of typical queries. Systems which hide these kinds of details are easier to understand (since they act more like single unit, with fewer details to think about), while systems that expose more real-world details may be more performant (because they correspond(符合) more closely to reality).

Several types of failures make writing distributed systems that act like a single system difficult. Network latency and network partitions (e.g. total network failure between some nodes) mean that a system needs to sometimes make hard choices about whether it is better to stay available but lose some crucial(关键的) guarantees that cannot be enforced, or to play it safe and refuse(拒绝) clients when these types of failures occur.

The CAP theorem - which I will discuss in the next chapter - captures some of these tensions. In the end, the ideal system meets both programmer needs (clean semantics) and business needs (availability/consistency/latency).

## Design techniques: partition and replicate(复制)

The manner in which a data set is distributed between multiple nodes is very important. In order for any computation to happen, we need to locate the data and then act on it.
数据集在多个节点之间的分布方式非常重要。为了进行任何计算，我们需要定位数据并对其进行操作。

There are two basic techniques that can be applied to a data set. It can be split over multiple nodes (partitioning) to allow for more parallel processing. It can also be copied or cached(缓存) on different nodes to reduce the distance between the client and the server and for greater fault tolerance (replication).

> Divide and conquer - I mean, partition and replicate.

The picture below illustrates the difference between these two: partitioned data (A and B below) is divided into independent sets, while replicated data (C below) is copied to multiple locations.

![Partition and replicate](http://book.mixu.net/distsys/images/part-repl.png)

This is the one-two punch(组合拳) for solving any problem where distributed computing plays a role. Of course, the trick is in picking the right technique for your concrete(具体的) implementation; there are many algorithms that implement replication and partitioning, each with different limitations and advantages which need to be assessed against your design objectives.

### Partitioning

Partitioning is dividing the dataset into smaller distinct independent sets; this is used to reduce the impact of dataset growth since each partition is a subset of the data.

- Partitioning improves performance by limiting the amount of data to be examined and by locating related data in the same partition
- Partitioning improves availability by allowing partitions to fail independently, increasing the number of nodes that need to fail before availability is sacrificed(牺牲)

Partitioning is also very much application-specific, so it is hard to say much about it without knowing the specifics. That's why the focus is on replication in most texts, including this one.

Partitioning is mostly about defining your partitions based on what you think the primary access pattern will be, and dealing with the limitations that come from having independent partitions (e.g. inefficient access across partitions, different rate of growth etc.).
分区主要是根据你认为的主要访问模式定义分区，并处理独立分区带来的限制（例如，跨分区的低效访问、不同的增长率等）.

### Replication

Replication is making copies of the same data on multiple machines; this allows more servers to take part in the computation.

Let me inaccurately(不准确地) quote [Homer J. Simpson](http://en.wikipedia.org/wiki/Homer_vs._the_Eighteenth_Amendment):

> To replication! The cause of, and solution to all of life's problems.

Replication - copying or reproducing something - is the primary way in which we can fight latency.

- Replication improves performance by making additional computing power and bandwidth(带宽) applicable(适用的) to a new copy of the data
- Replication improves availability by creating additional copies of the data, increasing the number of nodes that need to fail before availability is sacrificed

Replication is about providing extra bandwidth, and caching where it counts. It is also about maintaining consistency in some way according to some consistency model.

Replication allows us to achieve scalability, performance and fault tolerance. Afraid of loss of availability or reduced performance? Replicate the data to avoid a bottleneck or single point of failure. Slow computation? Replicate the computation on multiple systems. Slow I/O? Replicate the data to a local cache to reduce latency or onto multiple machines to increase throughput.

Replication is also the source of many of the problems, since there are now independent copies of the data that has to be kept in sync on multiple machines - this means ensuring that the replication follows a consistency model.

The choice of a consistency model is crucial: a good consistency model provides clean semantics for programmers (in other words, the properties it guarantees are easy to reason about) and meets business/design goals such as high availability or strong consistency.

Only one consistency model for replication - strong consistency - allows you to program as-if the underlying data was not replicated. Other consistency models expose some internals of the replication to the programmer. However, weaker consistency models can provide lower latency and higher availability - and are not necessarily harder to understand, just different.
只有一种一致性模型（强一致性）允许你像底层数据没有被复制一样进行编程。其他的一致性模型向程序员公开了复制的一些内部内容。然而，较弱的一致性模型可以提供较低的延迟和较高的可用性，并且不一定很难理解，只是不同而已。

---

## Further reading

- [The Datacenter as a Computer - An Introduction to the Design of Warehouse-Scale Machines](http://www.morganclaypool.com/doi/pdf/10.2200/s00193ed1v01y200905cac006) - Barroso &  Hölzle, 2008
- [Fallacies of Distributed Computing](http://en.wikipedia.org/wiki/Fallacies_of_Distributed_Computing)
- [Notes on Distributed Systems for Young Bloods](http://www.somethingsimilar.com/2013/01/14/notes-on-distributed-systems-for-young-bloods/) - Hodges, 2013
