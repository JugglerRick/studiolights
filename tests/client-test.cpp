#include <iostream>

#include <boost/lexical_cast.hpp>
#include <boost/fusion/adapted.hpp>

#define BOOST_LOG_DYN_LINK 1
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup.hpp>

#include <restc-cpp/restc-cpp.h>
#include <restc-cpp/RequestBuilder.h>

using namespace std;
using namespace restc_cpp;
namespace logging = boost::log;

// C++ structure that match the JSON entries received
// from http://jsonplaceholder.typicode.com/posts/{id}
struct Post {
    int userId = 0;
    int id = 0;
    string title;
    string body;
};

// Since C++ does not (yet) offer reflection, we need to tell the library how
// to map json members to a type. We are doing this by declaring the
// structs/classes with BOOST_FUSION_ADAPT_STRUCT from the boost libraries.
// This allows us to convert the C++ classes to and from JSON.

BOOST_FUSION_ADAPT_STRUCT(
    Post,
    (int, userId)
    (int, id)
    (string, title)
    (string, body)
)

// The C++ main function - the place where any adventure starts
int main() {
    // Set the log-level to a reasonable value
    logging::core::get()->set_filter( logging::trivial::severity >= logging::trivial::info );

    // Create an instance of the rest client
    auto rest_client = RestClient::Create();

    // Create and instantiate a Post from data received from the server.
    Post my_post = rest_client->ProcessWithPromiseT<Post>([&](Context& ctx) {
        // Instantiate a Post structure.
        Post post;
        SerializeFromJson(post,
           RequestBuilder(ctx)
                .Get("http://jsonplaceholder.typicode.com/posts/1")
                .Header("X-Client", "RESTC_CPP")
                .Header("X-Client-Purpose", "Testing")
                .Execute());
        return post;
        }
    ).get();

    // Print the result for everyone to see.
    cout << "Received post# " << my_post.id << ", title: " << my_post.title
        << endl;
}
