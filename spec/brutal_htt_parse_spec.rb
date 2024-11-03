# frozen_string_literal: true

require "base64"
require "securerandom"
RSpec.describe BrutalHTTParse::Parser do
  let(:host) { "example.com" }
  let(:port) { "80" }
  let(:sec_websocket_key) { Base64.encode64(SecureRandom.random_bytes(16)).strip }
  let(:request) do
    [
      "GET / HTTP/1.1",
      "Host: #{host}:#{port}",
      "Upgrade: websocket",
      "Connection: Upgrade",
      "Sec-WebSocket-Key: #{sec_websocket_key}",
      "Sec-WebSocket-Version: 13",
      "\r\n"
    ].join("\r\n")
    # [
    #   "GET / HTTP/1.1",
    #   "Host: #{host}:#{port}",
    #   "\r\n"
    # ].join("\r\n")
  end

  let(:parser_class) do
    Class.new(described_class) do
      %i[on_url
         on_status
         on_method
         on_version
         on_header_field
         on_header_value
         on_chunk_extension_name
         on_chunk_extension_value
         on_body].each do |method_name|
        define_method(method_name) do |data|
          pp "#{method_name}: #{data}"
        end
      end

      def on_headers_complete
        puts "on_headers_complete"
      end
    end
  end
  subject { parser_class.new }

  it "does something useful" do
    subject
    expect(subject).to be_kind_of(BrutalHTTParse::Parser)
    # puts Process.pid
    # STDIN.gets.chomp == "c"
    pp subject.execute(request)
  end
end
