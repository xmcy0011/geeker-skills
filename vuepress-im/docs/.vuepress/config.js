const { description } = require("../../package");

module.exports = {
  /**
   * Ref：https://v1.vuepress.vuejs.org/config/#title
   */
  title: "IM开发知识体系",
  /**
   * Ref：https://v1.vuepress.vuejs.org/config/#description
   */
  description: description,

  /**
   * Extra tags to be injected to the page HTML `<head>`
   *
   * ref：https://v1.vuepress.vuejs.org/config/#head
   */
  head: [
    ["meta", { name: "theme-color", content: "#3eaf7c" }],
    ["meta", { name: "apple-mobile-web-app-capable", content: "yes" }],
    [
      "meta",
      { name: "apple-mobile-web-app-status-bar-style", content: "black" },
    ],
  ],

  /**
   * Theme configuration, here is the default theme configuration for VuePress.
   *
   * ref：https://v1.vuepress.vuejs.org/theme/default-theme-config.html
   */
  themeConfig: {
    repo: "",
    editLinks: false,
    docsDir: "",
    editLinkText: "",
    lastUpdated: false,
    nav: [
      {
        text: "Guide",
        link: "/guide/",
      },
      {
        text: "Interview",
        link: "/interview/",
      },
      {
        text: "Programming",
        link: "/programming/",
      },
      {
        text: "IM",
        link: "/im/",
      },
      {
        text: "Geek265.com",
        link: "https://www.geek265.com",
      },
    ],
    sidebar: {
      "/guide/": [["", "Guide"]],
      "/interview/": [
        {
          title: "基础",
          collapsable: false,
          children: [
            ["/interview/base/operate system.md", "操作系统"],
            ["/interview/base/database.md", "数据库"],
            ["/interview/base/network.md", "网络"],
            ["/interview/base/data structure.md", "数据结构"],
            ["/interview/base/algorithm.md", "算法"],
          ],
        },
        {
          title: "语言",
          collapsable: false,
          children: [
            ["/interview/program language/cpp.md", "C++"],
            ["/interview/program language/go.md", "Go"],
            ["/interview/program language/java.md", "Java"],
          ],
        },
        {
          title: "后端必备",
          collapsable: false,
          children: [
            ["/interview/backend/redis.md", "redis"],
            ["/interview/backend/message queue.md", "mq"],
          ],
        },
        {
          title: "领域",
          collapsable: false,
          children: [
            ["/interview/domain/stream media server/", "流媒体"],
            ["/interview/domain/audio and video/", "音视频"],
            ["/interview/domain/instant message/", "IM开发"],
          ],
        },
      ],
      "/programming/": [["", "程序员练级攻略"]],
      "/im/": [["", "IM开发"]],
      // {
      //   title: "" ,
      //   collapsable: true,
      //   children:[]
      // }
    },
  },

  /**
   * Apply plugins，ref：https://v1.vuepress.vuejs.org/zh/plugin/
   */
  plugins: ["@vuepress/plugin-back-to-top", "@vuepress/plugin-medium-zoom"],
};
