// 2020/08/28 - Created by netcan: https://github.com/netcan
#pragma once
#include "../core/flow_builder.hpp"
#include "task_analyzer.hpp"
#include "job_trait.hpp"

namespace tf {
template<typename ...Links>
class TaskDsl {
    using AllJobs = typename TaskAnalyzer<Links...>::AllJobs;
    using JobsCb = typename Map_t<AllJobs, JobCb>::template exportTo<std::tuple>;

    using OneToOneLinkSet = typename TaskAnalyzer<Links...>::OneToOneLinkSet;
    template <typename OneToOneLink>
    struct OneToOneLinkInstanceType
    { using type = typename OneToOneLink::template InstanceType<JobsCb>; };
    using OneToOneLinkInstances =
        typename Map_t<OneToOneLinkSet, OneToOneLinkInstanceType>::template exportTo<std::tuple>;
public:
    constexpr TaskDsl(FlowBuilder& flow_builder) {
        build_jobs_cb(flow_builder, std::make_index_sequence<AllJobs::size>{});
        build_links(std::make_index_sequence<OneToOneLinkSet::size>{});
    }
private:
    template<size_t ...Is>
    void build_jobs_cb(FlowBuilder& flow_builder, std::index_sequence<Is...>) {
        (std::get<Is>(jobsCb_).build(flow_builder), ...);
    }

    template<size_t ...Is>
    void build_links(std::index_sequence<Is...>) {
        (std::get<Is>(links_).build(jobsCb_), ...);
    }
private:
    JobsCb jobsCb_;
    OneToOneLinkInstances links_;
};

#define __some_job(...) tf::SomeJob<__VA_ARGS__>
#define __fork(...) __some_job(__VA_ARGS__)
#define __merge(...) auto(__some_job(__VA_ARGS__))
#define __link(Job) auto(Job)
#define __taskbuild(...) tf::TaskDsl<__VA_ARGS__>
#define __def_task(name, ...) struct name { \
    auto operator()() __VA_ARGS__                  \
}

}
